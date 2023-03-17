import {
  Badge,
  Button,
  Card,
  Flex,
  Group,
  Image,
  ScrollArea,
  Stack,
  Text,
  TextInput,
} from "@mantine/core";
import { observer } from "mobx-react";
import { FC, useContext, useState } from "react";
import { TbSearch } from "react-icons/tb";
import { ParametersContext } from "../juceIntegration/JuceIntegration";
import { setProperty } from "../juceIntegration/messages/pluginMessages";

interface Model {
  name: string;
  descriptionShort: string;
  descriptionLong: string;
  tags: string[];
}

interface ModelCardProps {
  model: Model;
}

export const ModelCard: FC<ModelCardProps> = observer(({ model }) => {
  const { name, descriptionShort, descriptionLong, tags } = model;

  return (
    <Card shadow="lg" padding="sm" radius="md" withBorder>
      <Card.Section>
        <Image src={`./images/${name}.jpg`} height={160} alt="Norway" />
      </Card.Section>
      <Stack mt="xs" mb="xs">
        <Text weight={500}>{name}</Text>
        <Group>
          {tags.map((tag) => (
            <Badge color="pink" variant="light">
              {tag}
            </Badge>
          ))}
        </Group>
      </Stack>

      <Text size="sm" color="dimmed">
        {descriptionShort}
      </Text>

      <Button
        variant="light"
        color="blue"
        fullWidth
        mt="md"
        radius="md"
        onClick={() => setProperty("currentModel", name)}
      >
        Use
      </Button>
    </Card>
  );
});

interface Props {}

export const ModelBrowser: FC<Props> = observer((props) => {
  // const [modelsTab, setModelsTab] = useState("myModels");
  const [searchText, setSearchText] = useState("");

  const parameters = useContext(ParametersContext)!;
  const modelList = parameters.valueTree.properties.get(
    "modelList"
  ) as string[];

  return (
    <Flex style={{ flex: 1 }} direction="column">
      {/* <Flex direction="column" p="sm">
        <SegmentedControl
          value={modelsTab}
          onChange={setModelsTab}
          data={[
            { label: "My Models", value: "myModels" },
            { label: "Shop", value: "shop" },
          ]}
        />
      </Flex> */}
      <TextInput
        placeholder="Search models"
        icon={<TbSearch size="0.8rem" />}
        m="sm"
        value={searchText}
        onChange={(e) => setSearchText(e.target.value)}
      />
      <ScrollArea style={{ flex: "1 1 auto" }} type="scroll" h={0} px="sm">
        <Stack pb="sm">
          {modelList!
            .map((modelJson) => JSON.parse(modelJson))
            .filter((json) => {
              if (searchText === "") return true;
              else if (
                (json.model_name as string).toLowerCase().includes(searchText)
              )
                return true;
              else if (
                (json.tags as string[]).filter((tag) =>
                  tag.toLowerCase().includes(searchText)
                ).length > 0
              )
                return true;
              return false;
            })
            .map((json) => {
              return (
                <ModelCard
                  model={{
                    name: json.model_name,
                    descriptionShort: json.model_short_description,
                    descriptionLong: json.model_long_description,
                    tags: json.tags,
                  }}
                />
              );
            })}
        </Stack>
      </ScrollArea>
    </Flex>
  );
});
